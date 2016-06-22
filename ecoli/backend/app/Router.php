<?php

namespace Ecoli;

class Router implements SingletonContract
{
	/**
	 * Core application object.
	 *
	 * @var Ecoli\Application
	 */
	static $app;

	/**
	 * The Artisan commands provided by your application.
	 *
	 * @var array
	 */
	protected $routes = [];

	/**
	 * @var Singleton The reference to *Singleton* instance of this class
	 */
	private static $instance;

	/**
	 * Returns the *Singleton* instance of this class.
	 *
	 * @return Singleton The *Singleton* instance.
	 */
	private function addRoute(Array $route)
	{
		array_push($this->routes, $route);
	}

	/**
	 * Returns the *Singleton* instance of this class.
	 *
	 * @return Singleton The *Singleton* instance.
	 */
	public static function getInstance()
	{
		if (null === static::$instance) {
			static::$instance = new static();
		}
		
		return static::$instance;
	}

	/**
	 * Register GET route.
	 *
	 * @param  string  $uri
	 * @param  string  $controller
	 * @return void
	 */
	static function get($uri, $controller)
	{
		$controlFunction = explode("::", $controller);
		if (count($controlFunction) != 2) {
			return;
		}

		static::getInstance()->addRoute([
			'uri' => $uri,
			'method' => 'get',
			'controler' => $controlFunction[0],
			'function' => $controlFunction[1],
		]);
	}

	/**
	 * Register POST route.
	 *
	 * @param  string  $uri
	 * @param  string  $controller
	 * @return void
	 */
	static function post($uri, $controller)
	{
		$controlFunction = explode("::", $controller);
		if (count($controlFunction) != 2) {
			return;
		}

		static::getInstance()->addRoute([
			'uri' => $uri,
			'method' => 'post',
			'controler' => $controlFunction[0],
			'function' => $controlFunction[1],
		]);
	}

	/**
	 * Match Request.
	 *
	 * @param  Ecoli\Request  $request
	 * @return Array
	 */
	public function match(Request $request)
	{
		foreach ($this->routes as $route) {
			if ($route['uri'] == $request->uri() && $route['method'] == $request->method()) {
				return [$route['controler'], $route['function']];
			}
		}

		return null;
	}

	/**
	 * Register any hook and run initializer routines.
	 *
	 * @return void
	 */
	public function boot()
	{
		
	}

	/**
	 * Inject core application.
	 *
	 * @return void
	 */
	public function inject($core)
	{
		$this->app = $core;
	}

	/**
	 * Protected constructor to prevent creating a new instance of the
	 * *Singleton* via the `new` operator from outside of this class.
	 */
	protected function __construct()
	{
		
	}

	/**
	 * Private clone method to prevent cloning of the instance of the
	 * *Singleton* instance.
	 *
	 * @return void
	 */
	private function __clone()
	{

	}

	/**
	 * Private unserialize method to prevent unserializing of the *Singleton*
	 * instance.
	 *
	 * @return void
	 */
	private function __wakeup()
	{

	}

}