# QtDependencyResolver

This repository is based on Sergey Pupko's QtDependencyResolver which can be found [here](https://github.com/SPupko/QtDependencyResolver), but with additional Scope capability just like [Google Guice's Scope](https://github.com/google/guice/wiki/Scopes). Until now, just Singleton Scope was added. But, additional Scope will be added another time if needed.

QtDependencyResolver is a simple, ready-to-use library, that provide you ability to make IoC via DIContainer in your solution.
DIContainer is based on Qt Meta-Object System and use invoke method to make a constructor injection.

You can bind your interfaces and base classes to a concrete implementation and retrieve instances later.

## Usage

### Bind

To bind your classes you must create a **DIContainer** instance and use the **Bind** method:
    
    DIContainer* resolver = new DIContainer();
    resolver->Bind<IUsersService, LocalUsersService>();
    resolver->Bind<IUsersRepository, MongoDBUsersRepository>();
	
	
**Bind** method receives 1 optional parameter with type of `DIContainer::Scope`. Currently, there are two available `DIContainer::Scope`:

	`DIContainer::NO_SCOPE` and
	
	`DIContainer::SINGLETON`
	
By default, **Bind** will use `DIContainer::NO_SCOPE` if no Scope specified, which means every __DIContainer__ resolve will return another new object.
	
By passing `DIContainer::Singleton` Scope, we will make sure the object resolved by __DIContainer__ will always return the same object (Singleton):

	DIContainer* resolver = new DIContainer();
    resolver->Bind<IUsersService, LocalUsersService>(DIContainer::SINGLETON);
	resolver->Bind<IUsersRepository, MongoDBUsersRepository>(DIContainer::SINGLETON);


### Resolve

Then for resolving your interfaces or base classes you must use the **Resolve** method:

    IUserService* userService = resolver->Resolve<IUsersService>();

**Note:** *Only constructors that are declared with the Q_INVOKABLE modifier are made available to be invoked using QMetaObject::invokeMethod() through the meta-object system.
For more details look on DIContainerTest.cpp in DependencyResolver-Tests library.*


## Resources

https://github.com/setiawanpai/QtDependencyResolver (Git Repository)

https://github.com/setiawanpai/QtDependencyResolver/issues (Bug Reports)
  
