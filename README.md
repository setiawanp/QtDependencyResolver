# QtDependencyResolver

This repository is based on Sergey Pupko's QtDependencyResolver which can be found [here](https://github.com/SPupko/QtDependencyResolver), but with additional Scope capability just like [Google Guice's Scope](https://github.com/google/guice/wiki/Scopes). Until now, just Singleton Scope was added. But, additional Scope will be added another time if needed.

QtDependencyResolver is a simple, ready-to-use library, that provide you ability to make IoC via DIContainer in your solution.
DIContainer is based on Qt Meta-Object System and use invoke method to make a constructor injection.

You can bind your interfaces and base classes to a concrete implementation and retrieve instances later.

## Usage

### Bind

There are two available **Scope**:
	`DIContainer::NO_SCOPE` and `DIContainer::SINGLETON`

To bind your classes you must create a **DIContainer** instance and use the **Bind** method:
    
    DIContainer* resolver = new DIContainer();
    resolver->Bind<IUsersService, LocalUsersService>();
    resolver->Bind<IUsersRepository, MongoDBUsersRepository>();
	
You could bind **Singleton** object by passing Scope `DIContainer::SINGLETON` to **Bind** method's first parameter:

	DIContainer* resolver = new DIContainer();
    resolver->Bind<IUsersService, LocalUsersService>(DIContainer::SINGLETON);
	resolver->Bind<IUsersRepository, MongoDBUsersRepository>(DIContainer::SINGLETON);

*(By passing `DIContainer::Singleton` Scope, we will make sure the object created by __DIContainer__ will always be the same object)*

*Note: If no Scope specified in Bind method's first parameter, __DIContainer__ will use `DIContainer::NO_SCOPE`*


### Resolve

Then for resolving your interfaces or base classes you must use the **Resolve** method:

    IUserService* userService = resolver->Resolve<IUsersService>();

**Note:** *Only constructors that are declared with the Q_INVOKABLE modifier are made available to be invoked using QMetaObject::invokeMethod() through the meta-object system.
For more details look on DIContainerTest.cpp in DependencyResolver-Tests library.*


## Resources

https://github.com/setiawanpai/QtDependencyResolver (Git Repository)

https://github.com/setiawanpai/QtDependencyResolver/issues (Bug Reports)
  
